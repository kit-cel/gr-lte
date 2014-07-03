

#ifndef INCLUDED_LTE_MIMO_PSS_FREQ_SYNC_H
#define INCLUDED_LTE_MIMO_PSS_FREQ_SYNC_H

#include <lte/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/analog/sig_source_c.h>

namespace gr {
  namespace lte {

    /*!
     * \brief <+description of block+>
     * \ingroup lte
     *
     */
    class LTE_API mimo_pss_freq_sync : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<mimo_pss_freq_sync> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of lte::mimo_pss_freq_sync.
       *
       * To avoid accidental use of raw pointers, lte::mimo_pss_freq_sync's
       * constructor is in a private implementation
       * class. lte::mimo_pss_freq_sync::make is the public interface for
       * creating new instances.
       */
      static sptr make(int fftl, int rxant, boost::shared_ptr<gr::analog::sig_source_c> &sig);
    };

  } // namespace lte
} // namespace gr

#endif /* INCLUDED_LTE_MIMO_PSS_FREQ_SYNC_H */

